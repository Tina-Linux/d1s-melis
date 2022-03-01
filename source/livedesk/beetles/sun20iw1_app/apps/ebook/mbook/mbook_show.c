/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              record app sample
*
*                                   (c) Copyright 2006-2009, China
*                                            All Rights Reserved
*
* File    : mbook_show.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/
#include <log.h>
#include "common/misc.h"
#include "mbook_private.h"

typedef struct tag_MBOOK_SHOW
{
    __mp            *mp_display;      // 显示驱动操作句柄
    __show_config_t     config;         // 配置信息
} __mbook_show_t;

/*
**********************************************************************************************************************
*                                               __show_page_utf8
*
* Description: 写一页到软图层上（UTF8编码格式）
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __show_page_utf8(__mbook_show_t *hdle, __hdle lyr, void *data)
{
    __u16                   tmp = 0;
    __u32                   tmp_w;
    __u32                   x, y;
    __u32                   row;
    __u8                   *p_data;
    __mbook_show_t         *p_show;
    __show_config_t        *p_config;
    p_data = data;
    p_show = hdle;
    p_config = &p_show->config;
    GUI_LyrWinSel(lyr);
    //  GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(p_config->char_font);
    //  GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0x0);
    GUI_SetColor(p_config->font_color);
    GUI_Clear();
    x = p_config->show_start;
    y = 0;
    row = 0;

    while (row < p_config->page_row)
    {
        if ((*p_data == 0x0a) || (*p_data == 0x0d))
        {
            //换行符检查
            row++;
            x = p_config->show_start;
            y = y + p_config->row_height;

            if (*(p_data + 1) == 0x0a)
            {
                p_data = p_data + 2;
            }
            else
            {
                p_data = p_data + 1;
            }

            continue;
        }
        else if ((tmp == 0xff) && ((*(p_data + 1) & 0xff) == 0xfe))
        {
            break;
        }
        else if ((*p_data & 0xf0) == 0xe0)
        {
            // 三字节流数据
            tmp = ((*p_data & 0x0f) << 12) | ((*(p_data + 1) & 0x3f) << 6) | (*(p_data + 2) & 0x3f);
            tmp_w = p_config->col_width;
            p_data = p_data + 3;
        }
        else if ((*p_data & 0xe0) == 0xc0)
        {
            // 双字节流数据
            tmp = ((*p_data & 0x3f) << 6) | (*(p_data + 1) & 0x3f);
            tmp_w = p_config->col_width;
            p_data = p_data + 2;
        }
        else
        {
            // 单字节流数据
            tmp = *p_data & 0xff;
            tmp_w = *(p_config->char_table + tmp);
            p_data = p_data + 1;
        }

        if ((x + tmp_w) > p_config->show_width + p_config->show_start)
        {
            row++;

            if (row >= p_config->page_row)
            {
                break;
            }

            x = p_config->show_start;
            y = y + p_config->row_height;
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
        else
        {
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __show_page_utf16_be
*
* Description: 写一页到软图层上（UTF16_BE编码格式）
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __show_page_utf16_be(__mbook_show_t *hdle, __hdle lyr, void *data)
{
    __u16                   tmp;
    __u32                   tmp_w;
    __u32                   x, y;
    __u32                   row;
    __u8                   *p_data;
    __mbook_show_t         *p_show;
    __show_config_t        *p_config;
    p_data = data;
    p_show = hdle;
    p_config = &p_show->config;
    GUI_LyrWinSel(lyr);
    //  GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(p_config->char_font);
    //  GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0x0);
    GUI_SetColor(p_config->font_color);
    GUI_Clear();
    x = p_config->show_start;
    y = 0;
    row = 0;

    while (row < p_config->page_row)
    {
        tmp = ((*p_data & 0xff) << 8) + (*(p_data + 1) & 0xff);

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            row++;
            x = p_config->show_start;
            y = y + p_config->row_height;

            if ((*(p_data + 3) & 0xff) == 0x0a)
            {
                p_data = p_data + 4;
            }
            else
            {
                p_data = p_data + 2;
            }

            continue;
        }
        else if (tmp == 0xfffe)
        {
            break;
        }
        else if (tmp < 0x80)
        {
            tmp_w = *(p_config->char_table + tmp);
            p_data = p_data + 2;
        }
        else
        {
            tmp_w = p_config->col_width;
            p_data = p_data + 2;
        }

        if ((x + tmp_w) > p_config->show_width + p_config->show_start)
        {
            row++;

            if (row >= p_config->page_row)
            {
                break;
            }

            x = p_config->show_start;
            y = y + p_config->row_height;
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
        else
        {
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __show_page_utf16_le
*
* Description: 写一页到软图层上（UTF16_LE编码格式）
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __show_page_utf16_le(__mbook_show_t *hdle, __hdle lyr, void *data)
{
    __u16                   tmp;
    __u32                   tmp_w;
    __u32                   x, y;
    __u32                   row;
    __u8                   *p_data;
    __mbook_show_t         *p_show;
    __show_config_t        *p_config;
    p_data = data;
    p_show = hdle;
    p_config = &p_show->config;
    GUI_LyrWinSel(lyr);
    //  GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(p_config->char_font);
    //  GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0x0);
    GUI_SetColor(p_config->font_color);
    GUI_Clear();
    x = p_config->show_start;
    y = 0;
    row = 0;

    while (row < p_config->page_row)
    {
        tmp = (*p_data & 0xff) + ((*(p_data + 1) & 0xff) << 8);

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            row++;
            x = p_config->show_start;
            y = y + p_config->row_height;

            if ((*(p_data + 2) & 0xff) == 0x0a)
            {
                p_data = p_data + 4;
            }
            else
            {
                p_data = p_data + 2;
            }

            continue;
        }
        else if (tmp == 0xfeff)
        {
            break;
        }
        else if (tmp < 0x80)
        {
            tmp_w = *(p_config->char_table + tmp);
            p_data = p_data + 2;
        }
        else
        {
            tmp_w = p_config->col_width;
            p_data = p_data + 2;
        }

        if ((x + tmp_w) > p_config->show_width + p_config->show_start)
        {
            row++;

            if (row >= p_config->page_row)
            {
                break;
            }

            x = p_config->show_start;
            y = y + p_config->row_height;
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
        else
        {
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __show_page_gbk
*
* Description: 写一页到软图层上（GBK编码格式）
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __show_page_gbk(__mbook_show_t *hdle, __hdle lyr, void *data)
{
    __u16                   tmp;
    __u32                   tmp_w;
    __u32                   x, y;
    __u32                   row;
    __u8                   *p_data;
    __mbook_show_t         *p_show;
    __show_config_t        *p_config;
    p_data = data;
    p_show = hdle;
    p_config = &p_show->config;
    GUI_LyrWinSel(lyr);
    GUI_SetFont(p_config->char_font);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(p_config->font_color);
    GUI_Clear();
    x = p_config->show_start;
    y = 0;
    row = 0;

    while (row < p_config->page_row)
    {
        tmp = *p_data & 0xff;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            row++;
            x = p_config->show_start;
            y = y + p_config->row_height;

            if ((*(p_data + 1) & 0xff) == 0x0a)
            {
                p_data = p_data + 2;
            }
            else
            {
                p_data = p_data + 1;
            }

            __wrn("row:%d p_config->row_height:%d  y:%d", row, p_config->row_height, y);
            continue;
        }
        else if (tmp == 0xff)
        {
            if ((*(p_data + 1) & 0xff) == 0xfe)
            {
                //__here__;
                break;
            }
            else
            {
                tmp_w = *(p_config->char_table + 0x3f);     // ?
                p_data = p_data + 1;
            }
        }
        else if (tmp > 0x7f)
        {
            tmp = (tmp << 8) + *(p_data + 1);
            {
                __u16 t;
                t = tmp;
                tmp = eLIBs_GBK_to_Unicode(t);

                if (0xffff == tmp)
                {
                    __wrn("eLIBs_GBK_to_Unicode fail, t = %x", t);
                }
            }
            tmp_w = p_config->col_width;
            p_data = p_data + 2;
        }
        else
        {
            tmp_w = *(p_config->char_table + tmp);
            p_data = p_data + 1;
        }

        if ((x + tmp_w) > p_config->show_width + p_config->show_start)
        {
            row++;

            if (row >= p_config->page_row)
            {
                break;
            }

            x = p_config->show_start;
            y = y + p_config->row_height;
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
        else
        {
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
    }

    return;
}

/*
**********************************************************************************************************************
*                                               __show_page_big5
*
* Description: 写一页到软图层上（GBK编码格式）
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __show_page_big5(__mbook_show_t *hdle, __hdle lyr, void *data)
{
    __u16                   tmp;
    __u32                   tmp_w;
    __u32                   x, y;
    __u32                   row;
    __u8                   *p_data;
    __mbook_show_t         *p_show;
    __show_config_t        *p_config;

    p_data = data;
    p_show = hdle;
    p_config = &p_show->config;
    GUI_LyrWinSel(lyr);
    //  GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(p_config->char_font);
    //  GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0x0);
    GUI_SetColor(p_config->font_color);
    GUI_Clear();
    x = p_config->show_start;
    y = 0;
    row = 0;

    while (row < p_config->page_row)
    {
        tmp = *p_data & 0xff;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            row++;
            x = p_config->show_start;
            y = y + p_config->row_height;

            if ((*(p_data + 1) & 0xff) == 0x0a)
            {
                p_data = p_data + 2;
            }
            else
            {
                p_data = p_data + 1;
            }

            continue;
        }
        else if (tmp == 0xff)
        {
            if ((*(p_data + 1) & 0xff) == 0xfe)
            {
                break;
            }
            else
            {
                tmp_w = *(p_config->char_table + 0x3f);     // ?
                p_data = p_data + 1;
            }
        }
        else if (tmp > 0x7f)
        {
            tmp = (tmp << 8) + *(p_data + 1);
            tmp = eLIBs_BIG5_to_Unicode(tmp);
            tmp_w = p_config->col_width;
            p_data = p_data + 2;
        }
        else
        {
            tmp_w = *(p_config->char_table + tmp);
            p_data = p_data + 1;
        }

        if ((x + tmp_w) > p_config->show_width + p_config->show_start)
        {
            row++;

            if (row >= p_config->page_row)
            {
                break;
            }

            x = p_config->show_start;
            y = y + p_config->row_height;
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
        else
        {
            GUI_DispCharAt(tmp, x, y);
            x = x + tmp_w;
        }
    }
}

/*
************************************************************************************************************************
*                                       MBOOK_Show_Init
*
*Description: 页面显示模块的初始化
*
*Arguments  :
*
*
*Return     : H_SHOW: 返回页面显示模块操作句柄
*
************************************************************************************************************************
*/
H_SHOW  MBOOK_Show_Init(void)
{
    __mbook_show_t          *p_show;
    p_show = (__mbook_show_t *)My_Malloc(0, sizeof(__mbook_show_t));    // 申请内存

    if (p_show == 0)
    {
        __wrn(" malloc error ");
        return NULL;
    }

    eLIBs_memset(p_show, 0, sizeof(__mbook_show_t));
	p_show->mp_display = esKSRV_Get_Display_Hld();

    if (p_show->mp_display == NULL)
    {
        goto SHOW_ERROR_1;
    }

    return p_show;
    /*
    SHOW_ERROR_2:
        eLIBs_fclose(p_show->mp_display);
        p_show->mp_display = NULL;
    */
SHOW_ERROR_1:
    My_Mfree(0, p_show);
    return NULL;
}

/*
************************************************************************************************************************
*                                       MBOOK_Show_Config
*
*Description: 页面显示模块的参数配置
*
*Arguments  : hdle: 操作句柄
*             config: 配置信息参数指针
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Show_Config(H_SHOW hdle, __show_config_t *config)
{
    __mbook_show_t          *p_show;

    if (hdle == NULL)
    {
        __wrn("MBOOK_Show_Config hdle == NULL");
        return EPDK_FAIL;
    }

    p_show = (__mbook_show_t *)hdle;
    eLIBs_memcpy(&(p_show->config), config, sizeof(__show_config_t));
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Show_Page
*
*Description: 写一页数据到指定的软图层上
*
*Arguments  : hdle: 操作句柄
*             lyr: 图层句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Show_Page(H_SHOW hdle, __hdle lyr, void *data)
{
    __mbook_show_t          *p_show;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_show = (__mbook_show_t *)hdle;
    __log("%s, p_show->config.charset:%d", __FUNCTION__, p_show->config.charset);

    switch (p_show->config.charset)
    {
        case EPDK_CHARSET_ENM_UTF8 :
        {
            __show_page_utf8(p_show, lyr, data);
        }
        break;

        case EPDK_CHARSET_ENM_UTF16BE:
        {
            __show_page_utf16_be(p_show, lyr, data);
        }
        break;

        case EPDK_CHARSET_ENM_UTF16LE:
        {
            __show_page_utf16_le(p_show, lyr, data);
        }
        break;

        case EPDK_CHARSET_ENM_GBK:
        {
            __show_page_gbk(p_show, lyr, data);

        }
        break;

        case EPDK_CHARSET_ENM_BIG5:
        {
            __show_page_big5(p_show, lyr, data);
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Show_Uninit
*
*Description: 删除页面显示模块
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Show_Uninit(H_SHOW hdle)
{
    __mbook_show_t          *p_show;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_show = (__mbook_show_t *)hdle;

    if (p_show->mp_display)
    {
        //eLIBs_fclose(p_show->mp_display);
        p_show->mp_display = NULL;
    }

    My_Mfree(0, p_show);
    return EPDK_OK;
}


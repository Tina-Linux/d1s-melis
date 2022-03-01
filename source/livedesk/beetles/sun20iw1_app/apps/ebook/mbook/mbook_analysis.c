/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              record app sample
*
*                                   (c) Copyright 2006-2009, China
*                                            All Rights Reserved
*
* File    : mbook_analysis.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/
#include <log.h>
#include "mbook_private.h"
#include "common/misc.h"


typedef struct tag_MBOOK_ANALYSIS
{
    ES_FILE            *file_hdle;      // 文件句柄
    __u32               block_size;     // 一个block的大小 4 byte
    __u32               block_len;      // 一个block的大小 4 byte

    __u8               *block_buf;      // block的内存地址
    __one_page_t       *page_info;      // page info的内存地址
    __one_page_t       *p_operate;      // 指向当前操作的info

    __u32               total_page;     // 总页码
    __u32               last_row;       // 上次退出时，结尾所在的行号
    __u32               last_start;     // 上次退出时，结尾所在行的偏移量
    __u32               last_end;       // 上次退出时，文件的偏移量

    __analysis_config_t     config;         // info的配置信息
} __mbook_analysis_t;


/*
************************************************************************************************************************
*                                       __analysis_page_utf8
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __analysis_page_utf8(__mbook_analysis_t *hdle, __u32 len)
{
    __u32                   tmp;
    __u32                   tmp_ofs;
    __u32                   tmp_row;
    __u32                   tmp_x;
    __u32                   tmp_w;
    __u8                   *p_data;
    __one_page_t           *p_one;
    __analysis_config_t    *p_config;
    __u8                    byte_num_of_per_char = 0 ;  //add by linyaoshu
    tmp_ofs = 0;
    tmp_row = hdle->last_row;
    tmp_x   = hdle->last_start;
    p_data  = hdle->block_buf;
    p_one   = hdle->p_operate;
    p_config = &hdle->config;

    while (1)
    {
        tmp = *(p_data + tmp_ofs) & 0xff;

        if ((tmp == 0x0a) || (tmp == 0x0d))
        {
            tmp_row++;      // 行数加1
            tmp_x = 0;

            if (*(p_data + tmp_ofs + 1) == 0x0a)
            {
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }
            else
            {
                tmp_ofs = tmp_ofs + 1;
                byte_num_of_per_char = 1 ;
            }

            if (tmp_row >= p_config->page_row)
            {
                hdle->total_page++;
                p_one->page_end = hdle->last_end + tmp_ofs;
                p_one->page_no = hdle->total_page;
                (p_one + 1)->page_start = p_one->page_end;
                p_one++;
                tmp_row = 0;
            }
        }
        else
        {
            if ((tmp & 0xf0) == 0xe0)
            {
                tmp_w = p_config->col_width;               // 行偏移相对起始位置1个汉字偏移量
                tmp_ofs = tmp_ofs + 3;
                byte_num_of_per_char = 3 ;
            }
            else if ((tmp & 0xe0) == 0xc0)
            {
                tmp_w = p_config->col_width;               // 行偏移相对起始位置1个汉字偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }
            else
            {
                tmp_w = *(p_config->char_table + tmp);      // 行偏移相对起始位置1个字符偏移量
                tmp_ofs = tmp_ofs + 1;
                byte_num_of_per_char = 1 ;
            }

            if ((tmp_x + tmp_w) > p_config->show_width) //满一行
            {
                tmp_row++;
                tmp_x = 0;
                tmp_ofs = tmp_ofs - byte_num_of_per_char;

                if (tmp_row >= p_config->page_row)
                {
                    hdle->total_page++;
                    p_one->page_end = hdle->last_end + tmp_ofs;
                    p_one->page_no  = hdle->total_page;
                    (p_one + 1)->page_start = p_one->page_end;
                    p_one++;
                    tmp_row = 0;
                }
            }
            else
            {
                tmp_x = tmp_x + tmp_w;
            }
        }

        if ((tmp_ofs + 3) >= len)
        {
            break;
        }

        if (hdle->total_page >= MAX_SHOW_PAGE - 2)
        {
            hdle->last_end  = hdle->last_end + tmp_ofs;
            hdle->p_operate = p_one;
            hdle->last_row   = tmp_row;
            hdle->last_start = tmp_x;
            return  EPDK_FAIL;
        }
    }

    hdle->last_end  = hdle->last_end + tmp_ofs;
    hdle->p_operate = p_one;
    hdle->last_row   = tmp_row;
    hdle->last_start = tmp_x;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_utf16_be
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __analysis_page_utf16_be(__mbook_analysis_t *hdle, __u32 len)
{
    __u32                   tmp;
    __u32                   tmp_ofs;
    __u32                   tmp_row;
    __u32                   tmp_x;
    __u32                   tmp_w;
    __u8                   *p_data;
    __one_page_t           *p_one;
    __analysis_config_t    *p_config;
    __u8                    byte_num_of_per_char = 0 ;  //add by linyaoshu
    tmp_ofs = 0;
    tmp_row = hdle->last_row;
    tmp_x   = hdle->last_start;
    p_data  = hdle->block_buf;
    p_one   = hdle->p_operate;
    p_config = &hdle->config;

    while (1)
    {
        tmp = ((*(p_data + tmp_ofs) & 0xff) << 8) + (*(p_data + tmp_ofs + 1) & 0xff);

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            tmp_row++;
            tmp_x = 0;

            if ((*(p_data + tmp_ofs + 3) & 0xff) == 0x0a)
            {
                tmp_ofs = tmp_ofs + 4;
                byte_num_of_per_char = 4 ;
            }
            else
            {
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2;
            }

            if (tmp_row >= p_config->page_row)
            {
                hdle->total_page++;
                p_one->page_end = hdle->last_end + tmp_ofs;
                p_one->page_no = hdle->total_page;
                (p_one + 1)->page_start = p_one->page_end;
                p_one++;
                tmp_row = 0;
            }
        }
        else
        {
            if (tmp < 0x80)
            {
                tmp_w   = *(p_config->char_table + tmp);    // 行偏移相对起始位置1个字符偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }
            else
            {
                tmp_w   = p_config->col_width;             // 行偏移相对起始位置1个汉字偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }

            if ((tmp_x + tmp_w) > p_config->show_width)
            {
                tmp_row++;
                tmp_x = 0;
                tmp_ofs = tmp_ofs - byte_num_of_per_char;

                if (tmp_row >= p_config->page_row)
                {
                    hdle->total_page++;
                    p_one->page_end = hdle->last_end + tmp_ofs;
                    p_one->page_no  = hdle->total_page;
                    (p_one + 1)->page_start = p_one->page_end;
                    p_one++;
                    tmp_row = 0;
                }
            }
            else
            {
                tmp_x = tmp_x + tmp_w;
            }
        }

        if ((tmp_ofs + 4) >= len)
        {
            break;
        }

        if (hdle->total_page >= MAX_SHOW_PAGE - 2)
        {
            hdle->last_end  = hdle->last_end + tmp_ofs;
            hdle->p_operate = p_one;
            hdle->last_row   = tmp_row;
            hdle->last_start = tmp_x;
            return  EPDK_FAIL;
        }
    }

    hdle->last_end  = hdle->last_end + tmp_ofs;
    hdle->p_operate = p_one;
    hdle->last_row = tmp_row;
    hdle->last_start = tmp_x;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_utf16_le
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __analysis_page_utf16_le(__mbook_analysis_t *hdle, __u32 len)
{
    __u32                   tmp;
    __u32                   tmp_ofs;
    __u32                   tmp_row;
    __u32                   tmp_x;
    __u32                   tmp_w;
    __u8                   *p_data;
    __one_page_t           *p_one;
    __analysis_config_t    *p_config;
    __u8                    byte_num_of_per_char = 0 ;  //add by linyaoshu
    tmp_ofs = 0;
    tmp_row = hdle->last_row;
    tmp_x   = hdle->last_start;
    p_data  = hdle->block_buf;
    p_one   = hdle->p_operate;
    p_config = &hdle->config;

    while (1)
    {
        tmp = (*(p_data + tmp_ofs) & 0xff) + ((*(p_data + tmp_ofs + 1) & 0xff) << 8);

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            tmp_row++;
            tmp_x = 0;

            if ((*(p_data + tmp_ofs + 2) & 0xff) == 0x0a)
            {
                tmp_ofs = tmp_ofs + 4;
                byte_num_of_per_char = 4 ;
            }
            else
            {
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }

            if (tmp_row >= p_config->page_row)
            {
                hdle->total_page++;
                p_one->page_end = hdle->last_end + tmp_ofs;
                p_one->page_no = hdle->total_page;
                (p_one + 1)->page_start = p_one->page_end;
                p_one++;
                tmp_row = 0;
            }
        }
        else
        {
            if (tmp < 0x80)
            {
                tmp_w   = *(p_config->char_table + tmp);        // 行偏移相对起始位置1个字符偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }
            else
            {
                tmp_w   = p_config->col_width;                 // 行偏移相对起始位置1个汉字偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2;
            }

            if ((tmp_x + tmp_w) > p_config->show_width)
            {
                tmp_row++;
                tmp_x = 0;
                tmp_ofs = tmp_ofs - byte_num_of_per_char;

                if (tmp_row >= p_config->page_row)
                {
                    hdle->total_page++;
                    p_one->page_end = hdle->last_end + tmp_ofs;
                    p_one->page_no  = hdle->total_page;
                    (p_one + 1)->page_start = p_one->page_end;
                    p_one++;
                    tmp_row = 0;
                }
            }
            else
            {
                tmp_x = tmp_x + tmp_w;
            }
        }

        if ((tmp_ofs + 4) >= len)
        {
            break;
        }

        if (hdle->total_page > MAX_SHOW_PAGE - 2)
        {
            hdle->last_end  = hdle->last_end + tmp_ofs;
            hdle->p_operate = p_one;
            hdle->last_row   = tmp_row;
            hdle->last_start = tmp_x;
            return  EPDK_FAIL;
        }
    }

    hdle->last_end  = hdle->last_end + tmp_ofs;
    hdle->p_operate = p_one;
    hdle->last_row = tmp_row;
    hdle->last_start = tmp_x;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_gbk
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __analysis_page_gbk(__mbook_analysis_t *hdle, __u32 len)
{
    __u32                   tmp;
    __u32                   tmp_ofs;
    __u32                   tmp_row;
    __u32                   tmp_x;
    __u32                   tmp_w;
    __u8                   *p_data;
    __one_page_t           *p_one;
    __analysis_config_t    *p_config;
    __u8                    byte_num_of_per_char = 0 ;  //add by linyaoshu
    tmp_ofs = 0;
    tmp_row = hdle->last_row;
    tmp_x   = hdle->last_start;
    p_data  = hdle->block_buf;
    p_one   = hdle->p_operate;
    p_config = &hdle->config;

    while (1)
    {
        tmp = *(p_data + tmp_ofs) & 0xff;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            tmp_row++;
            tmp_x = 0;

            if ((*(p_data + tmp_ofs + 1) & 0xff) == 0x0a)
            {
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }
            else
            {
                tmp_ofs = tmp_ofs + 1;
                byte_num_of_per_char = 1 ;
            }

            if (tmp_row >= p_config->page_row)
            {
                hdle->total_page++;
                p_one->page_end = hdle->last_end + tmp_ofs;
                p_one->page_no = hdle->total_page;
                (p_one + 1)->page_start = p_one->page_end;
                p_one++;
                tmp_row = 0;
            }
        }
        else
        {
            if (tmp > 0x7f)
            {
                tmp_w   = p_config->col_width;                 // 行偏移相对起始位置1个汉字偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2 ;
            }
            else
            {
                tmp_w   = *(p_config->char_table + tmp);        // 行偏移相对起始位置1个字符偏移量
                tmp_ofs = tmp_ofs + 1;
                byte_num_of_per_char = 1 ;
            }

            if ((tmp_x + tmp_w) > p_config->show_width)         // 满一行
            {
                tmp_row++;
                tmp_x = 0;
                tmp_ofs = tmp_ofs - byte_num_of_per_char;

                if (tmp_row >= p_config->page_row)          //满一页
                {
                    hdle->total_page++;
                    p_one->page_end = hdle->last_end + tmp_ofs;
                    p_one->page_no  = hdle->total_page;
                    (p_one + 1)->page_start = p_one->page_end;  // p_one+1,指向向下一页
                    p_one++;
                    tmp_row = 0;
                }
            }
            else
            {
                tmp_x = tmp_x + tmp_w;
            }
        }

        if ((tmp_ofs + 4) >= len)
        {
            break;
        }

        if (hdle->total_page >= MAX_SHOW_PAGE - 2)
        {
            hdle->last_end  = hdle->last_end + tmp_ofs;
            hdle->p_operate = p_one;
            hdle->last_row   = tmp_row;
            hdle->last_start = tmp_x;
            return  EPDK_FAIL;
        }
    }

    hdle->last_end  = hdle->last_end + tmp_ofs;
    hdle->p_operate = p_one;
    hdle->last_row = tmp_row;
    hdle->last_start = tmp_x;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_big5
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __analysis_page_big5(__mbook_analysis_t *hdle, __u32 len)
{
    __u32                   tmp;
    __u32                   tmp_ofs;
    __u32                   tmp_row;
    __u32                   tmp_x;
    __u32                   tmp_w;
    __u8                   *p_data;
    __one_page_t           *p_one;
    __analysis_config_t    *p_config;
    __u8                    byte_num_of_per_char = 0 ;  //add by linyaoshu
    tmp_ofs = 0;
    tmp_row = hdle->last_row;
    tmp_x   = hdle->last_start;
    p_data  = hdle->block_buf;
    p_one   = hdle->p_operate;
    p_config = &hdle->config;

    while (1)
    {
        tmp = *(p_data + tmp_ofs) & 0xff;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            tmp_row++;
            tmp_x = 0;

            if ((*(p_data + tmp_ofs + 1) & 0xff) == 0x0a)
            {
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2;
            }
            else
            {
                tmp_ofs = tmp_ofs + 1;
                byte_num_of_per_char = 1;
            }

            if (tmp_row >= p_config->page_row)
            {
                hdle->total_page++;
                p_one->page_end = hdle->last_end + tmp_ofs;
                p_one->page_no = hdle->total_page;
                (p_one + 1)->page_start = p_one->page_end;
                p_one++;
                tmp_row = 0;
            }
        }
        else
        {
            if (tmp > 0x7f)
            {
                tmp_w   = p_config->col_width;                 // 行偏移相对起始位置1个汉字偏移量
                tmp_ofs = tmp_ofs + 2;
                byte_num_of_per_char = 2;
            }
            else
            {
                tmp_w   = *(p_config->char_table + tmp);        // 行偏移相对起始位置1个字符偏移量
                tmp_ofs = tmp_ofs + 1;
                byte_num_of_per_char = 1;
            }

            if ((tmp_x + tmp_w) > p_config->show_width)
            {
                tmp_row++;
                tmp_x = 0;
                tmp_ofs = tmp_ofs - byte_num_of_per_char;

                if (tmp_row >= p_config->page_row)
                {
                    hdle->total_page++;
                    p_one->page_end = hdle->last_end + tmp_ofs;
                    p_one->page_no  = hdle->total_page;
                    (p_one + 1)->page_start = p_one->page_end;
                    p_one++;
                    tmp_row = 0;
                }
            }
            else
            {
                tmp_x = tmp_x + tmp_w;
            }
        }

        if ((tmp_ofs + 4) >= len)
        {
            break;
        }

        if (hdle->total_page >= MAX_SHOW_PAGE - 2)
        {
            hdle->last_end  = hdle->last_end + tmp_ofs;
            hdle->p_operate = p_one;
            hdle->last_row   = tmp_row;
            hdle->last_start = tmp_x;
            return  EPDK_FAIL;
        }
    }

    hdle->last_end  = hdle->last_end + tmp_ofs;
    hdle->p_operate = p_one;
    hdle->last_row = tmp_row;
    hdle->last_start = tmp_x;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_info_utf8
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __analysis_page_info_utf8(__mbook_analysis_t *hdle)
{
    __u32                   len;
    __mbook_analysis_t     *p_analysis;
    __s32   ret ;
    p_analysis = hdle;

    while (1)
    {
        // 定位文件指针
        eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, SEEK_SET);
        // 读一段数据
        len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);
        ret = __analysis_page_utf8(hdle, len);

        if (EPDK_FAIL == ret)
        {
            __wrn("__analysis_page_utf8 fail\n");
            break ;
        }

        if (len < p_analysis->block_len)
        {
            p_analysis->total_page++;
            p_analysis->p_operate->page_no = p_analysis->total_page;
            p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
            break;
        }
    }

    p_analysis->p_operate = p_analysis->page_info;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_info_utf16_be
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __analysis_page_info_utf16_be(__mbook_analysis_t *hdle)
{
    __u32                   len;
    __mbook_analysis_t     *p_analysis;
    __s32   ret ;
    p_analysis = hdle;

    while (1)
    {
        // 定位文件指针
        eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, SEEK_SET);
        // 读一段数据
        len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);
        ret = __analysis_page_utf16_be(hdle, len);

        if (EPDK_FAIL == ret)
        {
            __wrn("__analysis_page_utf16_be fail\n");
            break ;
        }

        if (len < p_analysis->block_len)
        {
            p_analysis->total_page++;
            p_analysis->p_operate->page_no = p_analysis->total_page;
            p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
            break;
        }
    }

    p_analysis->p_operate = p_analysis->page_info;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_info_utf16_le
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __analysis_page_info_utf16_le(__mbook_analysis_t *hdle)
{
    __u32                   len;
    __mbook_analysis_t     *p_analysis;
    __s32   ret ;
    p_analysis = hdle;

    while (1)
    {
        // 定位文件指针
        eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, SEEK_SET);
        // 读一段数据
        len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);
        ret = __analysis_page_utf16_le(hdle, len);

        if (EPDK_FAIL == ret)
        {
            __wrn("__analysis_page_utf16_le fail\n");
            break ;
        }

        if (len < p_analysis->block_len)
        {
            p_analysis->total_page++;
            p_analysis->p_operate->page_no = p_analysis->total_page;
            p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
            break;
        }
    }

    p_analysis->p_operate = p_analysis->page_info;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_info_gbk
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __analysis_page_info_gbk(__mbook_analysis_t *hdle)
{
    __u32                   len;
    __mbook_analysis_t     *p_analysis;
    __s32   ret ;
    p_analysis = hdle;

    while (1)
    {
        // 定位文件指针
        eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, SEEK_SET);
        // 读一段数据
        len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);
        ret = __analysis_page_gbk(hdle, len);

        if (EPDK_FAIL == ret)
        {
            __wrn("__analysis_page_gbk fail\n");
            break ;
        }

        if (len < p_analysis->block_len)
        {
            p_analysis->total_page++;
            p_analysis->p_operate->page_no = p_analysis->total_page;
            p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
            break;
        }
    }

    p_analysis->p_operate = p_analysis->page_info;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       __analysis_page_info_big5
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __analysis_page_info_big5(__mbook_analysis_t *hdle)
{
    __u32                   len;
    __mbook_analysis_t     *p_analysis;
    __s32   ret ;
    p_analysis = hdle;

    while (1)
    {
        // 定位文件指针
        eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, SEEK_SET);
        // 读一段数据
        len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);
        ret = __analysis_page_big5(hdle, len);

        if (EPDK_FAIL == ret)
        {
            __wrn("__analysis_page_big5 fail\n");
            break ;
        }

        if (len < p_analysis->block_len)
        {
            p_analysis->total_page++;
            p_analysis->p_operate->page_no = p_analysis->total_page;
            p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
            break;
        }
    }

    p_analysis->p_operate = p_analysis->page_info;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_Init
*
*Description: 初始化页面分析模块
*
*Arguments  : fd: 文件操作句柄.
*             len: 文件总长度
*
*Return     : H_ANALYSIS: 返回PAGE操作句柄
*
************************************************************************************************************************
*/
H_ANALYSIS   MBOOK_Analysis_Init(ES_FILE *fd)
{
    __mbook_analysis_t     *p_analysis;
    __msg("MBOOK_Analysis_Init start \n");
    p_analysis = (__mbook_analysis_t *)My_Malloc(0, sizeof(__mbook_analysis_t));    // 申请内存

    if (p_analysis == 0)
    {
        __wrn(" malloc error \n");
        return NULL;
    }

    eLIBs_memset(p_analysis, 0, sizeof(__mbook_analysis_t));
    p_analysis->file_hdle = fd;
    p_analysis->block_size = 1 * 1024;
    p_analysis->block_len  = 1 * 1024 * 1024;
    p_analysis->block_buf = (void *)My_Palloc(p_analysis->block_size, 0);       // 申请内存

    if (p_analysis->block_buf == 0)
    {
        __wrn(" alloc error \n");
        goto ERROR_1;
    }

    p_analysis->page_info = (void *)My_Palloc((sizeof(__one_page_t) * MAX_SHOW_PAGE + 1023) / 1024, 0);     // 申请内存 2 M

    if (p_analysis->page_info == 0)
    {
        __wrn(" alloc error \n");
        goto ERROR_2;
    }

    eLIBs_memset(p_analysis->page_info, 0, (sizeof(__one_page_t) * MAX_SHOW_PAGE + 1023) / 1024 * 1024);
    p_analysis->p_operate = p_analysis->page_info;
    p_analysis->p_operate->page_no = 0;
    p_analysis->p_operate->page_start = 0;
    p_analysis->last_end    = 0;
    __msg("MBOOK_Analysis_Init finish \n");
    return p_analysis;
ERROR_2:

    if (p_analysis->block_buf)
    {
        My_Pfree(p_analysis->block_buf, p_analysis->block_size);
        p_analysis->block_buf = NULL;
    }

ERROR_1:

    if (p_analysis)
    {
        My_Mfree(0, p_analysis);
        p_analysis = NULL;
    }

    return NULL;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_Config
*
*Description: 配置页面分析模块相关参数
*
*Arguments  : hdle: 操作句柄
*             config: 参数配置信息指针
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Analysis_Config(H_ANALYSIS hdle, __analysis_config_t *config)
{
    __mbook_analysis_t *p_analysis;

    if (hdle == NULL)
    {
        __wrn("MBOOK_Analysis_Config hdle null \r\n");
        return EPDK_FAIL;
    }

    p_analysis = (__mbook_analysis_t *)hdle;
    eLIBs_memcpy(&(p_analysis->config), config, sizeof(__analysis_config_t));
    p_analysis->last_row    = 0;
    //p_analysis->last_start  = 2 * p_analysis->config.col_width;       // modify by linyaoshu 2012.02.02 14:27. txt文本本身开头空两格，否则不空
    p_analysis->last_start  = 0;

    switch (p_analysis->config.charset)
    {
        case EPDK_CHARSET_ENM_UTF8 :
        {
            p_analysis->p_operate->page_start = 3;
            p_analysis->last_end    = 3;
        }
        break;

        case EPDK_CHARSET_ENM_UTF16BE:
        case EPDK_CHARSET_ENM_UTF16LE:
        {
            p_analysis->p_operate->page_start = 2;
            p_analysis->last_end    = 2;
        }
        break;

        default:
        {
            p_analysis->p_operate->page_start = 0;
            p_analysis->last_end    = 0;
        }
        break;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_Work
*
*Description: 分析当前文件的页面信息
*
*Arguments  : hdle: 操作句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Analysis_Work(H_ANALYSIS hdle)
{
    __mbook_analysis_t     *p_analysis;

    if (hdle == NULL)
    {
        __wrn("MBOOK_Analysis_Work hdle == NULL \r\n");
        return EPDK_FAIL;
    }

    p_analysis = (__mbook_analysis_t *)hdle;
    p_analysis->total_page  = 0;
    p_analysis->p_operate   = p_analysis->page_info;
    p_analysis->p_operate->page_no = 0;
    p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
    __wrn("p_analysis->config.charset=%d\n", p_analysis->config.charset);

    switch (p_analysis->config.charset)
    {
        case EPDK_CHARSET_ENM_UTF8 :
        {
            __analysis_page_info_utf8(p_analysis);
        }
        break;

        case EPDK_CHARSET_ENM_UTF16BE:
        {
            __analysis_page_info_utf16_be(p_analysis);
        }
        break;

        case EPDK_CHARSET_ENM_UTF16LE:
        {
            __analysis_page_info_utf16_le(p_analysis);
        }
        break;

        case EPDK_CHARSET_ENM_GBK:
        {
            __analysis_page_info_gbk(p_analysis);
        }
        break;

        case EPDK_CHARSET_ENM_BIG5:
        {
            __analysis_page_info_big5(p_analysis);
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_GetInfo
*
*Description: 获取一页的页面信息。
*
*Arguments  : hdle: 操作句柄.
*             page_no: 要获取的页面号
*             page_info: 页面信息存放的地址
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Analysis_GetInfo(H_ANALYSIS hdle, __u32 page_no, __one_page_t *page_info)
{
    __one_page_t           *p_one;
    __mbook_analysis_t     *p_analysis;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_analysis = (__mbook_analysis_t *)hdle;

    if ((page_no == 0) || (page_no > p_analysis->total_page))
    {
        return EPDK_FAIL;
    }

    p_one = p_analysis->page_info + page_no - 1;
    eLIBs_memcpy(page_info, p_one, sizeof(__one_page_t));
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_GetPage
*
*Description: 获取指定偏移量对应的页面号。
*
*Arguments  : hdle: 操作句柄.
*             offset: 文件偏移量
*
*Return     : total_page: 成功，返回当前文件页码
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Analysis_GetPage(H_ANALYSIS hdle, __u32 offset)
{
    __u32                   i;
    __one_page_t           *p_one = NULL;
    __mbook_analysis_t     *p_analysis;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_analysis = (__mbook_analysis_t *)hdle;

    if (offset < 10)
    {
        return 1;
    }

    if (offset >= ((p_analysis->page_info + p_analysis->total_page - 1)->page_end))
    {
        return 1;
    }

    for (i = 0; i < p_analysis->total_page; i++)
    {
        p_one = p_analysis->page_info + i;

        if ((p_one->page_start <= offset) && (offset < p_one->page_end))
        {
            break;
        }
    }

    return p_one->page_no;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_GetTotalPage
*
*Description: 删除页面分析模块
*
*Arguments  : hdle: 操作句柄.
*
*Return     : total_page: 成功，返回当前文件总页码
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Analysis_GetTotalPage(H_ANALYSIS hdle)
{
    __mbook_analysis_t     *p_analysis;

    if (hdle == NULL)
    {
        __wrn("%s hdle == NULL\r\n", __FUNCTION__);
        return EPDK_FAIL;
    }

    p_analysis = (__mbook_analysis_t *)hdle;
    return p_analysis->total_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Analysis_Uninit
*
*Description: 删除页面分析模块
*
*Arguments  : hdle: 操作句柄.
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Analysis_Uninit(H_ANALYSIS hdle)
{
    __mbook_analysis_t     *p_analysis;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_analysis = (__mbook_analysis_t *)hdle;

    if (p_analysis->block_buf)
    {
        My_Pfree(p_analysis->block_buf, p_analysis->block_size);
        p_analysis->block_buf = NULL;
    }

    if (p_analysis->page_info)
    {
        My_Pfree(p_analysis->page_info, (sizeof(__one_page_t) * MAX_SHOW_PAGE + 1023) / 1024);
        p_analysis->page_info = NULL;
        p_analysis->p_operate = NULL;
    }

    My_Mfree(0, p_analysis);
    return EPDK_OK;
}




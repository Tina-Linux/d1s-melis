/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              FM app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : mbook_analysis.h
* By      : lyn
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
*/
#ifndef __MBOOK_ANALYSIS_H__
#define __MBOOK_ANALYSIS_H__

#define     MAX_SHOW_PAGE       100000

typedef     void   *H_ANALYSIS;

typedef struct tag_ONE_PAGE     // 页：12 byte
{
    __u32       page_no;        //页号
    __u32       page_start;     //起始位置 文件偏移量
    __u32       page_end;       //结束位置 文件偏移量
} __one_page_t;

typedef struct tag_INFO_CONFIG
{
    __u32       show_width;     // 文本显示的宽度
    __u32       page_row;       // 一页可显示的行数
    __u32       col_width;      // 列与列间的宽度
    __s32      *char_table;     // 字符字宽表

    __epdk_charset_enm_e    charset;
} __analysis_config_t;


H_ANALYSIS  MBOOK_Analysis_Init(ES_FILE *fd);

__s32   MBOOK_Analysis_Config(H_ANALYSIS hdle, __analysis_config_t *config);

__s32   MBOOK_Analysis_Work(H_ANALYSIS hdle);

__s32   MBOOK_Analysis_GetInfo(H_ANALYSIS hdle, __u32 page_no, __one_page_t *page_info);

__s32   MBOOK_Analysis_GetPage(H_ANALYSIS hdle, __u32 offset);

__s32   MBOOK_Analysis_GetTotalPage(H_ANALYSIS hdle);

__s32   MBOOK_Analysis_Uninit(H_ANALYSIS hdle);


#endif /* __MBOOK_ANALYSIS_H__ */

